static int run(const CommandLineOptions& options)
{
	IR::Module irModule;

	// Load the module.
	if(!loadModule(options.filename, irModule)) { return EXIT_FAILURE; }
	if(options.onlyCheck) { return EXIT_SUCCESS; }

	// Compile the module.
	Runtime::Module* module = nullptr;
	if(!options.precompiled) { module = Runtime::compileModule(irModule); }
	else
	{
		const UserSection* precompiledObjectSection = nullptr;
		for(const UserSection& userSection : irModule.userSections)
		{
			if(userSection.name == "wavm.precompiled_object")
			{
				precompiledObjectSection = &userSection;
				break;
			}
		}

		if(!precompiledObjectSection)
		{
			Log::printf(Log::error, "Input file did not contain 'wavm.precompiled_object' section");
			return EXIT_FAILURE;
		}
		else
		{
			module = Runtime::loadPrecompiledModule(irModule, precompiledObjectSection->data);
		}
	}

	// Link the module with the intrinsic modules.
	Compartment* compartment = Runtime::createCompartment();
	Context* context = Runtime::createContext(compartment);
	RootResolver rootResolver(compartment);

	Emscripten::Instance* emscriptenInstance = nullptr;
	if(options.enableEmscripten)
	{
		emscriptenInstance = Emscripten::instantiate(compartment, irModule);
		if(emscriptenInstance)
		{
			rootResolver.moduleNameToInstanceMap.set("env", emscriptenInstance->env);
			rootResolver.moduleNameToInstanceMap.set("asm2wasm", emscriptenInstance->asm2wasm);
			rootResolver.moduleNameToInstanceMap.set("global", emscriptenInstance->global);
		}
	}

	if(options.enableThreadTest)
	{
		ModuleInstance* threadTestInstance = ThreadTest::instantiate(compartment);
		rootResolver.moduleNameToInstanceMap.set("threadTest", threadTestInstance);
	}

	LinkResult linkResult = linkModule(irModule, rootResolver);
	if(!linkResult.success)
	{
		Log::printf(Log::error, "Failed to link module:\n");
		for(auto& missingImport : linkResult.missingImports)
		{
			Log::printf(Log::error,
						"Missing import: module=\"%s\" export=\"%s\" type=\"%s\"\n",
						missingImport.moduleName.c_str(),
						missingImport.exportName.c_str(),
						asString(missingImport.type).c_str());
		}
		return EXIT_FAILURE;
	}

	// Instantiate the module.
	ModuleInstance* moduleInstance = instantiateModule(
		compartment, module, std::move(linkResult.resolvedImports), options.filename);
	if(!moduleInstance) { return EXIT_FAILURE; }

	// Call the module start function, if it has one.
	FunctionInstance* startFunction = getStartFunction(moduleInstance);
	if(startFunction) { invokeFunctionChecked(context, startFunction, {}); }

	if(options.enableEmscripten)
	{
		// Call the Emscripten global initalizers.
		Emscripten::initializeGlobals(context, irModule, moduleInstance);
	}

	// Look up the function export to call.
	FunctionInstance* functionInstance;
	if(!options.functionName)
	{
		functionInstance = asFunctionNullable(getInstanceExport(moduleInstance, "main"));
		if(!functionInstance)
		{ functionInstance = asFunctionNullable(getInstanceExport(moduleInstance, "_main")); }
		if(!functionInstance)
		{
			Log::printf(Log::error, "Module does not export main function\n");
			return EXIT_FAILURE;
		}
	}
	else
	{
		functionInstance
			= asFunctionNullable(getInstanceExport(moduleInstance, options.functionName));
		if(!functionInstance)
		{
			Log::printf(Log::error, "Module does not export '%s'\n", options.functionName);
			return EXIT_FAILURE;
		}
	}
	FunctionType functionType = getFunctionType(functionInstance);

	// Set up the arguments for the invoke.
	std::vector<Value> invokeArgs;
	if(!options.functionName)
	{
		if(functionType.params().size() == 2)
		{
			if(!emscriptenInstance)
			{
				Log::printf(
					Log::error,
					"Module does not declare a default memory object to put arguments in.\n");
				return EXIT_FAILURE;
			}
			else
			{
				std::vector<const char*> argStrings;
				argStrings.push_back(options.filename);
				char** args = options.args;
				while(*args) { argStrings.push_back(*args++); };

				wavmAssert(emscriptenInstance);
				Emscripten::injectCommandArgs(emscriptenInstance, argStrings, invokeArgs);
			}
		}
		else if(functionType.params().size() > 0)
		{
			Log::printf(Log::error,
						"WebAssembly function requires %" PRIu64
						" argument(s), but only 0 or 2 can be passed!",
						functionType.params().size());
			return EXIT_FAILURE;
		}
	}
	else
	{
		for(U32 i = 0; options.args[i]; ++i)
		{
			Value value;
			switch(functionType.params()[i])
			{
			case ValueType::i32: value = (U32)atoi(options.args[i]); break;
			case ValueType::i64: value = (U64)atol(options.args[i]); break;
			case ValueType::f32: value = (F32)atof(options.args[i]); break;
			case ValueType::f64: value = atof(options.args[i]); break;
			case ValueType::v128:
			case ValueType::anyref:
			case ValueType::anyfunc:
				Errors::fatalf("Cannot parse command-line argument for %s function parameter",
							   asString(functionType.params()[i]));
			default: Errors::unreachable();
			}
			invokeArgs.push_back(value);
		}
	}

	// Invoke the function.
	Timing::Timer executionTimer;
	IR::ValueTuple functionResults = invokeFunctionChecked(context, functionInstance, invokeArgs);
	Timing::logTimer("Invoked function", executionTimer);

	if(options.functionName)
	{
		Log::printf(Log::debug,
					"%s returned: %s\n",
					options.functionName,
					asString(functionResults).c_str());
		return EXIT_SUCCESS;
	}
	else if(functionResults.size() == 1 && functionResults[0].type == ValueType::i32)
	{
		return functionResults[0].i32;
	}
	else
	{
		return EXIT_SUCCESS;
	}
}
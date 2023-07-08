const exec = require('async-execute');

/**
 * List the output lines from command
 * @return {string[]}
 */
module.exports = async function list(command) {
	const output = await exec(command);

	return output
		.split('\n')
		.map(item => item.trim())
		.filter(Boolean)
	;
};

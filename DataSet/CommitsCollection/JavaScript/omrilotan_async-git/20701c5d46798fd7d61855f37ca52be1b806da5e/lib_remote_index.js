const exec = require('async-execute');

/**
 * Get remote repo name and owner
 * @return {string{}}
 */
module.exports = async function remote() {
	const origin = await exec('git remote get-url origin');
	const nosuffix = origin.replace(/\.git$/, '');
	const [ match ] = nosuffix.match(/[\w-]*\/[\w-]+$/) || [ nosuffix ];

	const [ name, owner = '' ] = match.split('/').reverse();

	return { name, owner };
};



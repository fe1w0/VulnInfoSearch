const spawn = require('../../helpers/spawn');

/**
 * Get remote repo name and owner
 * @return {string{}}
 */
module.exports = async function remote() {
	const origin = await spawn('remote get-url origin');
	const nosuffix = origin.replace(/\.git$/, '');
	const [ match ] = nosuffix.match(/[\w-]*\/[\w-]+$/) || [ nosuffix ];

	const [ name, owner = '' ] = match.split('/').reverse();

	return { name, owner };
};



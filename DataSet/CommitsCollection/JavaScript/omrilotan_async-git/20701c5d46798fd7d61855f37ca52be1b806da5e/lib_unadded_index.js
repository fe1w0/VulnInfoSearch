const exec = require('async-execute');

/**
 * List all files that would be added by "add ."
 * @return {string[]}
 */
module.exports = async function unadded() {
	const output = await exec('git add --dry-run .');

	return output
		.split('\n')
		.map(extract)
		.filter(Boolean)
	;
};

/**
 * Get the file name from the verb description
 * @param  {string} line
 * @return {string}
 *
 * @example
 * extract("add 'file.txt'") // "file.txt"
 */
function extract(line) {
	const [
		verb, // eslint-disable-line no-unused-vars
		...path
	] = line.split(' ');

	return path.join(' ').replace(/^'|'$/g, '').trim();
}

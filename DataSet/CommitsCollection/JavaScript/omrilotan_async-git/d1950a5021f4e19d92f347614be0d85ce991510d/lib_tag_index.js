const spawn = require('../../helpers/spawn');

/**
 * Create and push a git tag with the last commit message
 * @param  {String} tag
 * @return {void}
 */
module.exports = async function(tag) {
	if (!tag || ![ 'string', 'number' ].includes(typeof tag)) {
		throw new TypeError(`string was expected, instead got ${tag}`);
	}

	const { message, author, email } = this;

	await Promise.all([
		spawn(`config user.name "${await author}"`),
		spawn(`config user.email "${await email}"`),
	]);
	await spawn(`tag -a ${tag} -m "${await message}"`);
	await spawn(`push origin refs/tags/${tag}`);
};

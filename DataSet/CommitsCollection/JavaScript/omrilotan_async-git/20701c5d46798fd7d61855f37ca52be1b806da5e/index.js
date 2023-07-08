const exec = require('async-execute');

const {
	list,
	modified,
	remote,
	reset,
	tag,
	unadded,
} = require('./lib');

/**
 * Items to pull out of git show entry
 * @type {Array[]}
 * @see https://git-scm.com/docs/git-show for placeholders
 */
const formats = [
	[ 'author', 'an' ],
	[ 'body', 'b' ],
	[ 'comitter', 'cn' ],
	[ 'email', 'ae' ],
	[ 'message', 'B' ],
	[ 'sha', 'H' ],
	[ 'short', 'h' ],
	[ 'subject', 's' ],
];

/**
 * Items to pull out of terminal output
 * @type {Array[]}
 */
const outputs = [
	[ 'branch', 'git rev-parse --abbrev-ref HEAD' ],
	[ 'origin', 'git remote get-url origin' ],
];

/**
 * Lists to get from multi-line terminal output
 * @type {Array[]}
 */
const lists = [
	[ 'changed', 'git diff-tree --no-commit-id --name-only -r HEAD' ],
	[ 'staged', 'git diff --name-only --cached' ],
	[ 'tags', 'git tag' ],
	[ 'unstaged', 'git diff --name-only' ],
	[ 'untracked', 'git ls-files -o --exclude-standard' ],
];

/**
 * Properties which will become (async) getters
 */
const getters = Object.assign(
	{
		date: async () => new Date(parseInt(await exec('git show -s --format=%at')) * 1000),
		name: async () => (await remote()).name,
		owner: async () => (await remote()).owner,
		unadded,
		version: async () => (await exec('git version')).split(' ').pop(),
	},
	...outputs.map(
		([ key, value ]) => ({ [key]: exec.bind(null, value) }),
	),
	...lists.map(
		([ key, value ]) => ({ [key]: list.bind(null, value) }),
	),
	...formats.map(
		([ key, value ]) => ({ [key]: exec.bind(null, `git show -s --format=%${value}`) }),
	),
);

/**
 * Properties which will become (async) functions
 */
const functions = {
	modified,
	reset,
	tag,
};

/**
 * @typedef     asyncGit
 * @description Get git info
 * @type     {Object}
 * @property {Promise<string>}   author    Author name of the last commit
 * @property {Promise<string>}   body      Most recent commit message body
 * @property {Promise<string>}   branch    Current branch name
 * @property {Promise<string>}   comitter  Comitter name of the last commit
 * @property {Promise<string>}   date      Get last author date
 * @property {Promise<string>}   email     Author email of the last commit
 * @property {Promise<string>}   message   Most recent commit full message
 * @property {Promise<string>}   name      Project name
 * @property {Promise<string>}   origin    Remote origin URL
 * @property {Promise<string>}   owner     Project owner
 * @property {Promise<string>}   sha       Unique identifier of the last commit
 * @property {Promise<string>}   short     7 Character Unique identifier of the last commit
 * @property {Promise<string>}   subject   Most recent commit subject
 * @property {Promise<string>}   version   Get git version (semver)
 * @property {Promise<string[]>} changed   List of files changed in the last commit
 * @property {Promise<string[]>} staged    List of staged files
 * @property {Promise<string[]>} unadded   List of files that would be added or removed by 'git add'
 * @property {Promise<string[]>} unstaged  List of unstaged files
 * @property {Promise<string[]>} untracked List of untracked files
 * @property {Function}          modified  Get the last modified date of a file
 * @property {Function}          reset     Reset current HEAD to the specified state
 * @property {Function}          tag       Create and push a git tag with the last commit message
 */
Object.defineProperties(
	module.exports,
	Object.assign(
		Object.entries(getters).reduce(
			(props, [ key, value ]) => Object.assign(
				props,
				{
					[key]: {
						get: value,
						configurable: true,
					},
				},
			),
			{},
		),
		Object.entries(functions).reduce(
			(props, [ key, value ]) => Object.assign(
				props,
				{
					[key]: {
						value,
						configurable: true,
					},
				},
			),
			{},
		),
	),
);

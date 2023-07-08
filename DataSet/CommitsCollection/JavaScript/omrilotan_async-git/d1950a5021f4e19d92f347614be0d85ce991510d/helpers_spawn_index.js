const { spawn } = require('child_process');

/**
 * Executes git with given arguments string. resolves with the output message
 * @param  {String} string
 * @return {Promise}
 */
module.exports = string => new Promise(
	(resolve, reject) => {
		const git = spawn(
			'git',
			string.split(' '),
		);

		const outputs = { stdout: [], stderr: [] };

		git.stdout.on(
			'data',
			data => outputs.stdout.push(data.toString()),
		);

		git.stderr.on(
			'data',
			data => outputs.stderr.push(data.toString()),
		);

		git.on('exit', code => {
			switch (code) {
				case 0:
					resolve(outputs.stdout.join('').trim());
					break;
				default:
					reject(outputs.stderr.join('').trim());
					break;
			}
		});
	},
);

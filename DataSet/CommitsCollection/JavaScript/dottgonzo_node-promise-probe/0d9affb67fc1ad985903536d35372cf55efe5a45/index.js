Object.defineProperty(exports, "__esModule", { value: true });
const child_process_1 = require("child_process");
const fs_1 = require("fs");
function ffprobe(file) {
    return new Promise((resolve, reject) => {
        if (!file)
            throw new Error('no file provided');
        fs_1.stat(file, (err, stats) => {
            if (err)
                return reject(new Error('wrong file provided'));
            child_process_1.exec('ffprobe -v quiet -print_format json -show_format -show_streams ' + file, (error, stdout, stderr) => {
                if (error)
                    return reject(error);
                if (!stdout)
                    return reject(new Error("can't probe file " + file));
                let ffprobed;
                try {
                    ffprobed = JSON.parse(stdout);
                }
                catch (err) {
                    return reject(err);
                }
                for (let i = 0; i < ffprobed.streams.length; i++) {
                    if (ffprobed.streams[i].codec_type === 'video')
                        ffprobed.video = ffprobed.streams[i];
                    if (ffprobed.streams[i].codec_type === 'audio' && ffprobed.streams[i].channels)
                        ffprobed.audio = ffprobed.streams[i];
                }
                resolve(ffprobed);
            });
        });
    });
}
exports.ffprobe = ffprobe;
function createMuteOgg(outputFile, options) {
    return new Promise((resolve, reject) => {
        if (!outputFile || !options || !options.seconds || !options.sampleRate || !options.numOfChannels)
            return reject(new Error('malformed props to createMuteOgg'));
        if (!Number.isInteger(options.seconds) ||
            !Number.isInteger(options.sampleRate) ||
            !Number.isInteger(options.numOfChannels))
            return reject(new Error('malformed numerico options prop for createMuteOgg'));
        const ch = options.numOfChannels === 1 ? 'mono' : 'stereo';
        child_process_1.exec('ffmpeg -f lavfi -i anullsrc=r=' +
            options.sampleRate +
            ':cl=' +
            ch +
            ' -t ' +
            options.seconds +
            ' -c:a libvorbis ' +
            outputFile, (error, stdout, stderr) => {
            if (error)
                return reject(error);
            resolve(true);
        });
    });
}
exports.createMuteOgg = createMuteOgg;
function cloneOggAsMuted(inputFile, outputFile, seconds) {
    return new Promise((resolve, reject) => {
        ffprobe(inputFile)
            .then(probed => {
            if (!seconds)
                seconds = parseInt(probed.format.duration);
            createMuteOgg(outputFile, {
                seconds: seconds,
                sampleRate: parseInt(probed.audio.sample_rate),
                numOfChannels: probed.audio.channels
            })
                .then(() => {
                resolve(true);
            })
                .catch(err => {
                reject(err);
            });
        })
            .catch(err => {
            reject(err);
        });
    });
}
exports.cloneOggAsMuted = cloneOggAsMuted;
//# sourceMappingURL=index.js.map
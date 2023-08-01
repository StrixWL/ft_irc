const { createLogger, format, transports, addColors } = require("winston");
const chalk = require("chalk");
const { DateTime } = require("luxon");

/* Handling date */
const date = () => DateTime.local();
const time = () => chalk.grey.italic(`[${date().toFormat("hh:mm:ss")}]`);

const loggerLevels = {
	levels: {
		error: 0,
		warn: 1,
		info: 2,
		verbose: 3,
		debug: 4
	},
	colors: {
		error: "bold red",
		warn: "bold yellow",
		info: "bold blue",
		verbose: "bold green",
		debug: "bold cyan"
	}
};
addColors(loggerLevels.colors);

/* Create the logger */
const { combine, errors, colorize, printf } = format;
const { Console, File } = transports;

const reg = /(warn)|(error)|(info)|(verbose)|(debug)/g;
const formatLevel = level => level.replace(reg, e => e.toUpperCase());

module.exports = createLogger({
	level: "debug",
	format: combine(
		colorize({ colors: loggerLevels.colors }),
		printf(
			({ level, message }) => `${time()} ${formatLevel(level)} ${message}`
		),
		errors({ stack: true })
	),
	transports: [
		new Console({ levels: loggerLevels.levels }),
	]
});


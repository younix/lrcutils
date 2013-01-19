lrcutils
========

Utils to play and record lyric files in
[lrc file format](http://en.wikipedia.org/wiki/LRC_\(file_format\)).

## lrcplay
lrcplay plays lrc files.
It opens a file and prints a line at the right time to STDOUT.

## lrcrec
lrcrec records timing tags in real time.
It opens a file and waits for a keystroke for every line.
After a keystroke from use user it prints a lyric line to STDOUT with a timing tag.

## bar
bar incrementally displays the last line from STDIN.
It may be used as a frontent for lrcplay.

# TODO
* manpages for all tools
* recording inline timing tags
* use offset in lrcplay
* swap from usleep() to nanosleep()
* integration into mpd (music player daemon)

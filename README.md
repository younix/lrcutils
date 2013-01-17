lrcutils
========

Utils to play and record lyric files in
[lrc file format](http://en.wikipedia.org/wiki/LRC_\(file_format\)).

## lrcplay
lrcplay plays lrc files.
It opens a file and prints a line at the right time to STDOUT.

## lrcrec
lrcrec records timing tags in real time.
It opens a file and wait for a return for every line.
After a return it prints a lyric line to STDOUT with a timing tag.

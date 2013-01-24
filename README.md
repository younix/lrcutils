lrcutils
========

Utils to play and record lyric files in
[lrc file format](http://en.wikipedia.org/wiki/LRC_\(file_format\)).
The lrc file format is interpreted as descriped in Wikipedia.

## lrcplay
lrcplay plays lrc files.
It opens a file and prints a line at the right time to STDOUT.
This tool also supports the enhanced lrc format with inline timing tags.

## lrcrec
lrcrec records timing tags in real time.
It opens a file and waits for a keystroke for every line.
After a keystroke from use user it prints a lyric line to STDOUT with a timing
tag.
lrcrec supports ID tags for metadata of lyric and song, like artist and title.

## bar
bar incrementally displays the last line from STDIN.
It may be used as a frontent for lrcplay.

# TODO
* manpages for all tools
* manpage which descripes the lrc file format
* recording inline timing tags
* use offset in lrcplay
* integration into mpd (music player daemon)
* add utf-8 support for lrcplay and lrcrec

# FAQ

## Where can I get lrc files for my favorite songs?

* http://www.lyrdb.com/karaoke/index.htm
* http://www.lyricsfly.com/

## Is it posible while recording to write output into file and at STDOUT at the same time?

lrcrec input.lrc | tee output.lrc

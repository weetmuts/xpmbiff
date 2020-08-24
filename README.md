# xpmbiff
Display an image inside an fvwm GoodStuff icon, to indicate if you have unready mail or not.

Fredrik Öhrström d92-foh@nada.kth.se
9 December 1995

I wanted xbiff to show in the buttonbar, however I did not want to have a
separate button to start my mailreader. Plus I wanted to have just as
beautiful xpm pixmaps as on the other buttons. Well, since I couldn't
find such a program I wrote one myself. Perhaps there is one already, well
here is mine anyway.

xpmbiff is a VERY simple mail notification program. It can display
a colored and shaped window which shows the status of the spoolfile.
It shows an empty icon if it is empty, otherwise a full icon.
When you click on the icon it will start a program of your choice,
probably a mail reader. When the spoolfile increase in size it will
execute another command of your choice, for example:
cat mail.au > /dev/audio

I have typed this in my fvwmrc file:
...
Style "xpmbiff" NoTitle, NoHandles, BorderWidth 0, StaysOnTop
...
*FvwmButtons - Whatever Swallow "xpmbiff" Exec bin/xpmbiff
bin/bitmaps/ettbrev.xpm bin/bitmaps/flerabrev.xpm 3 'xterm -e pine &'
'cat /usr/local/hacks/sounds/effects/youvegotmail.au > /dev/audio' &
...

xpmbiff is in $HOME/bin the bitmaps in $HOME/bin/bitmaps

This is the first version, try it and use it if you like. Please mail me
any comments and suggestions. It is made for being swallowed by GoodStuff
that is why it sets its position to 2000,2000. The pixmaps have to be
the same size.

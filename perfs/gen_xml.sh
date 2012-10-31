#!/bin/bash

# generates a plugin-settings.xml for custom-charts in TeamCity
# such a .xml must generated every time the models in perfs.def change
# this script assumes gen_xml.pl to be in the current working directory
# usage:
#		./gen_xml.sh > plugin-settings.xml
#   scp plugin-settings.xml forge@teamcity-systeme.lip6.fr:~/.BuildServer/config/ITS\ -\ Instantiable\ Transition\ Systems\ tools\ \&\ LibDDD/
#
# This is the project config directory on teamcity server as of 10/31/2012
# Note: zsh on teamcity may not like the big ugly path. If so, scp the .xml onto the server, then move it to the right place

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
echo "<settings>"
echo "  <custom-graphs>"
cat perfs.def | grep -v "its-reach" | ./gen_xml.pl
echo "  </custom-graphs>"
echo "</settings>"

#!/bin/sh

function usage() 
{
  echo "autodoc.sh [-v 1 -v 2]"
  exit 1
}

function to_txt() 
{
  html=$1
  text=$2
  cat $html | sed -e 's/<\/h.>/<hr>&/g' \
    | html2text -ascii -style pretty | tail -n+9 | head -n-3 > $text

}

# Syntax highlight languages that are unsupported by Doxygen
function highlight_extra()
{
  html=$1
  name=$2
  synx=$3
  doxy="doc/tmp/${html}.dox"
  echo '/**' > ${doxy}
  echo "\page ${html} ${name}" >> ${doxy}
  echo "\htmlonly" >> ${doxy}
  echo '<div class="fragment"><pre class="fragment">' >> ${doxy}
  source-highlight -n -s ${synx} < ${name} >> ${doxy}
  echo '</div></pre>' >> ${doxy}
  echo "\endhtmlonly" >> ${doxy}
  echo "*/" >> ${doxy}
}

path=`dirname $0`

v=0
while getopts "v:" i; do
  case "$i" in
    "v") v=${OPTARG};
    ;;
  esac
done

echo 'Highlighting Unsupported Files'
highlight_extra 'wht_Makefile_am' 'wht/Makefile.am' 'am'

echo 'Rebuilding Doxygen (use -v to see output)'

case ${v} in
  0)
    doxygen 1> /dev/null 2> /dev/null
  ;;
  1)
    doxygen 1> /dev/null 
  ;;
  2)
    doxygen
  ;;
  *)
    usage;
  ;;
esac;

echo 'Converting HTML to Text'
to_txt doc/html/readme.html README
to_txt doc/html/authors.html AUTHORS
to_txt doc/html/install.html INSTALL

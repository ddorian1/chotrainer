cd $(dirname $(readlink -f $0))

inkscape -z -e 16.png -w 16 -h 16 $1
inkscape -z -e 24.png -w 24 -h 24 $1
inkscape -z -e 32.png -w 32 -h 32 $1
inkscape -z -e 48.png -w 48 -h 48 $1
inkscape -z -e 64.png -w 64 -h 64 $1
inkscape -z -e 128.png -w 128 -h 128 $1
inkscape -z -e 256.png -w 256 -h 256 $1

convert *.png icon.ico

rm 16.png
rm 24.png
rm 32.png
rm 48.png
rm 64.png
rm 128.png
rm 256.png

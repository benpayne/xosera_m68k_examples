echo "Copying to" $1

cp layers.bin test.raw test.pal.raw atari_logo.raw atari_logo.pal.raw $1

diskutil eject $2

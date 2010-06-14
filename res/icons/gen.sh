THEME=/usr/share/icons/oxygen
SIZES="128x128 64x64 48x48 32x32 22x22 16x16 8x8"
ICONS=$(egrep 'fromTheme\("([a-z\-]+)"\)' ../../* -R | cut -d'"' -f2)

for i in $SIZES; do
    mkdir -p $i
    for j in $ICONS; do
	find $THEME/$i -name "$j.png" -exec cp {} $i \; -exec echo {} \;
    done
done

echo '<!DOCTYPE RCC><RCC version="1.0">' > icons.qrc
echo '<qresource prefix="icons">' >> icons.qrc

rcc -project | grep -v ./gen.sh | grep -v qresource | grep -v RCC  >> icons.qrc

echo '</qresource>' >> icons.qrc
echo '</RCC>' >> icons.qrc

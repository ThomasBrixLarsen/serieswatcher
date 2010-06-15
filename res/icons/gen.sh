THEME=/usr/share/icons/oxygen
NAME=serieswatcher
ICONS=$(egrep 'fromTheme\("([a-z\-]+)"\)' ../../* -R | cut -d'"' -f2)

#ROOT=$(echo $THEME | sed 's/\//\\\//g')
OLDPWD=$(pwd)

for j in $ICONS; do
    #find $THEME/$i -name "$j.png" -exec cp {} $i \; -exec echo {} \;
    cd $THEME
    DIRS=$(find . -name "$j.png")
    cd $OLDPWD
    for i in $DIRS; do
	mkdir -p ${NAME}/$(dirname $i)
	cp -uv $THEME/$i ${NAME}/$i
    done
done

cp -uv $THEME/index.theme ${NAME}


echo '<!DOCTYPE RCC><RCC version="1.0">' > icons.qrc
echo '<qresource prefix="icons">' >> icons.qrc

rcc -project | grep -v ./gen.sh | grep -v qresource | grep -v RCC | grep -v icons.qrc >> icons.qrc

echo '</qresource>' >> icons.qrc
echo '</RCC>' >> icons.qrc

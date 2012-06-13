#!/bin/sh

TULIP_DIR=$1
DEST_DIR=$2
SRC_DIR=$(pwd)

echo 'Wiping previous bundles into '$DEST_DIR
rm -fr $DEST_DIR
mkdir -p $DEST_DIR/application
cd $DEST_DIR/application
mkdir .background
cp $SRC_DIR/background.png .background/
rm -fr *
echo 'Creating base directories'
mkdir -p Tulip.app/Contents/{MacOS,Resources,Frameworks,PlugIns,SharedSupport}
cd Tulip.app/Contents
cp "$SRC_DIR/Info.plist" .
cp "$SRC_DIR/tulip.icns" "Resources/"
mkdir MacOS/tulip
cd MacOS/tulip
echo 'Copying binaries'
cp -r $TULIP_DIR/* .
rm -fr macos
cd ..
cp -r $TULIP_DIR/macos/* .
echo 'Generating DMG'
cd $DEST_DIR
sh $SRC_DIR/make_dmg.sh "Tulip" "4.0" $DEST_DIR/application
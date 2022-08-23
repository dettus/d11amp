#!/bin/sh

export VERSION=01
rm -rf d11amp_0.${VERSION}
rm -rf *.html *.log
mkdir d11amp_0.${VERSION}
cp -r ../../mark3/software/* d11amp_0.${VERSION}/
(
  cd d11amp_0.${VERSION}
  make clean
  make all
  make clean
)
tar cvfj d11amp_0.${VERSION}.tar.bz2 d11amp_0.${VERSION}/
ln d11amp_0.${VERSION}.tar.bz2 d11amp_latest.tar.bz2
rm -rf d11amp_0.${VERSION} 
mkdir  d11amp_0.${VERSION}
ls -l d11amp_latest.tar.bz2


python3 mkhtml.py

echo "Great! Now please run "
echo "  python3 upload.py"
echo "  cp old_releases.html ../website_template/"


md5sum d11amp_0.${VERSION}.tar.bz2



#!/bin/sh

export VERSION=59
export SOFTWARE_ROOT=../../../../mark4/software


date
mkdir package
(
  cd package
  mkdir d11amp_0.$VERSION
  cd d11amp_0.$VERSION

  echo "creating directories"
  for I in `grep /$ ../../files.mft`
  do
    mkdir -p $I
  done
  echo "copying files"
  for I in `grep -v /$ ../../files.mft`
  do
    cp $SOFTWARE_ROOT/$I  $I
  done

  cd ../
  tar cvfj ../d11amp_0.${VERSION}.tar.bz2 d11amp_0.${VERSION}/
)

ls -l d11amp_0.${VERSION}.tar.bz2
ln -f d11amp_0.${VERSION}.tar.bz2 d11amp_latest.tar.bz2

(
  echo "checking package"
  mkdir tmp
  cd tmp
  tar xvfj ../d11amp_0.${VERSION}.tar.bz2
  cd d11amp_0.${VERSION}
  make
  make SHA256_CMD=sha256sum check || make check
)
rm -rf tmp/

#python3 mkhtml.py
#
#echo "Great! Now please run "
#echo "  python3 2_upload.py"
#echo "  cp old_releases.html ../templates/"
#
#
md5sum d11amp_0.${VERSION}.tar.bz2 || md5 d11amp_0.${VERSION}.tar.bz2
md5sum d11amp_latest.tar.bz2 || md5 d11amp_latest.tar.bz2
date

echo "When you are satisfied, please run "
echo " python3 2_mkhtml.py 0.${VERSION}"
echo " python3 3_upload.py 0.${VERSION}"

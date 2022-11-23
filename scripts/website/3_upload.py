#/usr/local/bin/env python3
import sys

versionnumstring=sys.argv[1]
import pysftp
from getpass import getpass


filename_tarbz2='d11amp_'+versionnumstring+'.tar.bz2'

files=['index.html', 'old_releases.html', 'd11amp_latest.tar.bz2', filename_tarbz2]

password=getpass()

with pysftp.Connection('ssh.strato.de', username='dettus.net', password=password) as sftp:
	with sftp.cd('d11amp'):             # temporarily chdir to public
		for f in files:
			print('uploading ['+f+']')
			sftp.put(f)


print("-------------------------------------------------------")
print("done! Please do not forget to run")
print("   cp old_releases.html templates/old_releases.html")
print("   git commit -a")
print("   git push")

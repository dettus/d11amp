#/usr/local/bin/env python3
versionnumstring='0.01'


from datetime import datetime;
import hashlib
def mymd5(fname):
    hash_md5 = hashlib.md5()
    with open(fname, "rb") as f:
        for chunk in iter(lambda: f.read(4096), b""):
            hash_md5.update(chunk)
    return hash_md5.hexdigest()



currentdate=datetime.now()
monthnames=['jan','feb','mar','apr','may','jun','jul','aug','sep','oct','nov','dec']
filename_tarbz2='d11amp_'+versionnumstring+'.tar.bz2'

d=currentdate.day;
m=monthnames[currentdate.month-1];
y=currentdate.year;
datestring=f'{d:02d}-{m:s}-{y:04d}'
versionstring='Version '+versionnumstring

md5sum_tarbz2=mymd5(filename_tarbz2)


print(versionstring)
print('please enter description')
print('------------------------')
description=input()
print('------------------------')


for bothfiles in ['index.html', 'old_releases.html']:

	file1 = open("templates/"+bothfiles, 'r') 
	Lines = file1.readlines() 
	file1.close()


	file2 = open(bothfiles,'w') 
	for line in Lines: 
		if (line.find('<!-- ')!=-1 and line.find('HERE%')!=-1):
			file2.write(line)
			t1=line.replace('<!-- ','').replace(' --!>','')
		else:
			t1=line


		t2=t1.replace('%VERSIONHERE%',versionstring).	\
		   replace('%VERSIONNUMHERE%',versionnumstring).	\
		   replace('%DATEHERE%',datestring).	\
		   replace('%DESCRIPTIONHERE%',description).		\
		   replace('%TARBZ2HERE%',filename_tarbz2).		\
		   replace('%MD5SUM_TARBZ2HERE%',md5sum_tarbz2).	\

		file2.write(t2)


	file2.close()

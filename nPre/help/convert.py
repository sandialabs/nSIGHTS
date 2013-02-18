#!/usr/bin/env python

import os
import subprocess
import pdb

for root, dirs, files in os.walk('.'):
    for name in files:
        if(name.endswith('bmp')):
            newname = name.split('.')[0] + '.jpg'
            print 'convert ',name,' ',newname
            subprocess.call(['convert',name,newname])
        if(name.endswith('.htm')):
            newname = name.split('.')[0] + '.tmp'
            inf = open(name,'r')
            outf = open(newname,'w')
            for line in inf:
                line = line.replace('.bmp','.jpg')
                outf.write(line)
            outf.close()
            inf.close()
            subprocess.call(['mv',newname,name])




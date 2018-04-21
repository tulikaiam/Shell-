import os
import re
import sys

name=sys.argv[1]
path=sys.argv[2]
#print name
#print path
#words = ['password', 'user', 'ip', 'key']
#words = ['Tanya']
words=[]
words.append(name)
rx = re.compile('|'.join(words))
#print(rx)

freqword=dict()

for root, dirs, files in os.walk("/home/tulika/Shell-/src"):
#for root, dirs, files in os.walk(path):
    for filename in files:
        with open(filename) as df:
            print filename
            data = df.read()
        for match in rx.finditer(data):
            # Use the MatchObject as you like
            #print match.string
            if(freqword.get(filename)== None):
            	freqword[filename]=1
            else:
            	freqword[filename]=freqword.get(filename)+1
print freqword

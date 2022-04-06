import ftplib
import sys

args = sys.argv

with ftplib.FTP() as ftp:
    ftp.connect(args[1], int(args[2]), int(50))
    ftp.set_pasv("true")
    ftp.login('', '')
    
    for f in args[4:]:
        with open(f, 'rb') as fp:
            ftp.storbinary('stor ' + args[3] + f, fp)
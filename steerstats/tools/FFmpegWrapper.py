import subprocess as sp
from subprocess import call
import sys


FFMPEG_BIN = "ffmpeg" # on Linux ans Mac OS
# FFMPEG_BIN = "ffmpeg.exe" # on Windows


def framesToVideo(frameDir, x, y, output):

    command = [FFMPEG_BIN,
            '-y',
            '-i', str(frameDir)+'frame%d.ppm',
            '-acodec', 'libfaac',
            '-ar', '44100',
            '-ab', '96k',
            '-coder', 'ac',
            '-me_range', '16',
            '-subq',  '5',
            '-sc_threshold', '40',
            '-vcodec',  'libx264',
            '-s',  str(x)+'x'+str(y),
            '-pix_fmt',  'yuv420p',
            str(output)+'.mp4']
    
    
    # print "Command: " + str(command)

    try: 
        call( command)
        
    except Exception as err:
                raise Exception('*******  There was an error calling ' + str(FFMPEG_BIN) +'  ******')
    return str(output)+'.mp4'
    

if __name__ == '__main__':
    
    #example Usage
    # ./encodeVideo <dir> <width_x_height>
    # ./encodeVideo ./frames/ 1200x800
    # print sys.argv
    frameDir = sys.argv[1]
    x = sys.argv[2]
    y = sys.argv[3]
    framesToVideo(frameDir, x, y)

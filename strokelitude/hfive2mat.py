#!/usr/bin/env python
import pkg_resources
import pylab
import numpy as np
import sys
import tables
import motmot.fview_ext_trig.easy_decode as easy_decode
import matplotlib.ticker as mticker
import scipy.io
import warnings
import os
import os.path
warnings.filterwarnings("ignore")

def main():
    h5_file_dir = os.path.join(os.environ['HOME'],'strokelitude_h5')
    h5_file_list = os.listdir(h5_file_dir)
    if not h5_file_list:
        print '%s directory empty -- nothing to do'%(h5_file_dir,)
        sys.exit(0)
    
    # Create mat file directory if is doesn't exist
    mat_file_dir = os.path.join(os.environ['HOME'],'strokelitude_mat')
    if not os.path.exists(mat_file_dir):
        os.mkdir(mat_file_dir)
    
    # Loop over input files
    for fname_h5 in h5_file_list:
        fname_base, fname_ext  = os.path.splitext(fname_h5)
        if not fname_ext == '.h5':
            continue
        fname_mat = '%s.mat'%(fname_base,) 
    
        fname_mat_full = os.path.join(mat_file_dir, fname_mat)
        fname_h5_full = os.path.join(h5_file_dir,fname_h5)
        if os.path.exists(fname_mat_full):
            continue

        print 'converting: %s  -- >  %s'%(fname_h5,fname_mat)
        
        h5 = tables.openFile(fname_h5_full,mode='r')
        
        stroke_data=h5.root.stroke_data[:]
        stroke_times = stroke_data['trigger_timestamp']
        
        time_data=h5.root.time_data[:]
        gain,offset,resids = easy_decode.get_gain_offset_resids(
            input=time_data['framestamp'],
            output=time_data['timestamp'])
        top = h5.root.time_data.attrs.top
        
        wordstream = h5.root.ain_wordstream[:]
        wordstream = wordstream['word'] # extract into normal numpy array
        
        r=easy_decode.easy_decode(wordstream,gain,offset,top)
        
        if r is not None:
            chans = r.dtype.fields.keys()
            chans.sort()
            chans.remove('timestamps')
            if 1:
                Vcc = h5.root.ain_wordstream.attrs.Vcc
                channel_names = h5.root.ain_wordstream.attrs.channel_names
            else:
                Vcc=3.3
            ADCmax = (2**10)-1
            analog_gain = Vcc/ADCmax
        else:
            chans = []
        names = h5.root.ain_wordstream.attrs.channel_names
        savedict = {}
        
        if r is not None:
            t0 = r['timestamps'][0]
            savedict = {'ADC_timestamp':r['timestamps']}
        else:
            t0 = 0
        
        # Write data to a .mat file
        savedict['data'] = {}
        savedict['data']['frame'] = stroke_data['frame']
        savedict['data']['triggerTimeStamp'] = stroke_times
        savedict['data']['processingTimeStamp'] = stroke_data['processing_timestamp']
        savedict['data']['leftWingAngle'] = stroke_data['left']
        savedict['data']['rightWingAngle'] = stroke_data['right'] 
        savedict['data']['leftAntennaAngle'] = stroke_data['left_antenna']
        savedict['data']['rightAntennaAngle'] = stroke_data['right_antenna']
        savedict['data']['headAngle'] = stroke_data['head']
        savedict['data']['pulseWidth'] = stroke_data['pulse_width']
        savedict['data']['pulseFrame'] = stroke_data['pulse_frame']
        
        if chans != []:
            analog_key_list = []
            for i, name in enumerate(names):
                ADC_data = r[chans[i]]*analog_gain
                savedict["ADC"+str(name)] = ADC_data
                analog_key_list.append("ADC"+str(name))
        scipy.io.savemat(fname_mat_full,savedict)
    
        h5.close()
    
# ------------------------------------------------------------------------------
if __name__ == '__main__':

    main()

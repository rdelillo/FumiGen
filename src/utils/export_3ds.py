import c4d
from c4d import gui

# Warning the default time should be at 0
# @TODO:
# - add path as argument 

def main():

    # for each frame render the associated 3DS file.
    # The view and animation must already be defined
    # through the GUI.

    doc = c4d.documents.GetActiveDocument()

    fps = doc.GetFps()
    start = doc.GetMinTime().GetFrame(fps)
    until  = doc.GetMaxTime().GetFrame(fps)
    
    # For each frame we save the associated 3DS file.
    for i in xrange(start, until):
        
        # Construct new name
        tmp_name = []
        tmp_name.append('/Users/Robin/Desktop/Scene_3ds/')
        tmp_name.append('scene.{0:04d}'.format(i))
        tmp_name.append('.3ds')
        name = ''.join(tmp_name)
        print 'Export : %r' % name

        # Move to frame
        doc.SetTime(c4d.BaseTime(i, fps))
        
        # Update GUI
        c4d.DrawViews(c4d.DRAWFLAGS_ONLY_ACTIVE_VIEW|c4d.DRAWFLAGS_NO_THREAD|c4d.DRAWFLAGS_NO_REDUCTION|c4d.DRAWFLAGS_STATICBREAK)
        # Update timeline
        c4d.GeSyncMessage(c4d.EVMSG_TIMECHANGED)
        
        # Save document
        e = c4d.documents.SaveDocument(
            doc = doc,
            name = name,
            saveflags = c4d.SAVEDOCUMENTFLAGS_0,
            format = c4d.FORMAT_3DSEXPORT
            )
        
        if not e:
            gui.MessageDialog("Unable to export frame %r" % i)
            return 
        else:
            print "Frame %r saved" % i

if __name__=='__main__':
    main()


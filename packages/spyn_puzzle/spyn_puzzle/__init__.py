import os
if os.name == 'nt':
    try:
        # os.add_dll_directory("C:\\msys64\\mingw64\\bin")
        for f in os.environ['PYTHONPATH'].split(';'):
            sep = ';' if os.name == 'nt' else ':'
            if f == '': continue
            try: os.add_dll_directory(f)
            except: pass
        from spinbind11_puzzle import *
    except Excpeiotn as e:
        raise Exception("Cannot load the spinbind11_puzzle package: " + str(e))
else:
    from spinbind11_puzzle import *

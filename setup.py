from distutils.core import setup
import py2exe

setup(
  options = {'py2exe': {'bundle_files': 1,
                        "compressed": 1}},
  console = ["ganho.py"],
  zipfile = None)

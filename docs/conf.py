import subprocess


project = 'serialMux'
author = 'Jeroen F.J. Laros'
copyright = '2021, {}'.format(author)

extensions = ['breathe']
breathe_projects = {'doxygen': 'xml'}
breathe_default_project = 'doxygen'
breathe_default_members = ['members']

master_doc = 'index'

highlight_language = 'none'
html_theme = 'sphinx_rtd_theme'


subprocess.call('doxygen', shell=True)

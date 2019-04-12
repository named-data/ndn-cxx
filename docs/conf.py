# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# http://www.sphinx-doc.org/en/master/config

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = u'ndn-cxx: NDN C++ library with eXperimental eXtensions'
copyright = u'Copyright © 2013-2019 Regents of the University of California.'
author = u'Named Data Networking Project'

# The short X.Y version
#version = ''

# The full version, including alpha/beta/rc tags
#release = ''

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''
# Else, today_fmt is used as the format for a strftime call.
today_fmt = '%Y-%m-%d'


# -- General configuration ---------------------------------------------------

# If your documentation needs a minimal Sphinx version, state it here.
#
needs_sphinx = '1.1'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.extlinks',
    'sphinx.ext.todo',
]

def addExtensionIfExists(extension):
    try:
        __import__(extension)
        extensions.append(extension)
    except ImportError:
        sys.stderr.write("Extension '%s' not found. "
                         "Some documentation may not build correctly.\n" % extension)

if sys.version_info[0] >= 3:
    addExtensionIfExists('sphinxcontrib.doxylink')

# sphinxcontrib.googleanalytics is currently not working with the latest version of Sphinx
# if os.getenv('GOOGLE_ANALYTICS', None):
#     addExtensionIfExists('sphinxcontrib.googleanalytics')

# The master toctree document.
master_doc = 'index'

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'named_data_theme'

# Add any paths that contain custom themes here, relative to this directory.
html_theme_path = ['.']

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']


# -- Options for LaTeX output ------------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    ('index', 'ndn-cxx-docs.tex', u'NDN C++ library with eXperimental eXtensions',
     author, 'manual'),
]


# -- Options for manual page output ------------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('manpages/ndnsec',              'ndnsec',              'NDN security toolkit',                         None, 1),
    ('manpages/ndnsec-cert-dump',    'ndnsec-cert-dump',    'export an NDN certificate',                    None, 1),
    ('manpages/ndnsec-cert-gen',     'ndnsec-cert-gen',     'create an NDN certificate for an identity',    None, 1),
    ('manpages/ndnsec-cert-install', 'ndnsec-cert-install', 'import an NDN certificate from a file',        None, 1),
    ('manpages/ndnsec-delete',       'ndnsec-delete',       'delete an NDN identity, key, or certificate',  None, 1),
    ('manpages/ndnsec-export',       'ndnsec-export',       'export an NDN certificate and its private key to a file', None, 1),
    ('manpages/ndnsec-get-default',  'ndnsec-get-default',  'show the default NDN identity, key, and certificate for the current user', None, 1),
    ('manpages/ndnsec-import',       'ndnsec-import',       'import an NDN certificate and its private key from a file', None, 1),
    ('manpages/ndnsec-key-gen',      'ndnsec-key-gen',      'generate an NDN key for an identity',          None, 1),
    ('manpages/ndnsec-list',         'ndnsec-list',         'list all known NDN identities, keys, and certificates', None, 1),
    ('manpages/ndnsec-set-default',  'ndnsec-set-default',  'change the default NDN identity, key, or certificate for the current user', None, 1),
    ('manpages/ndnsec-sign-req',     'ndnsec-sign-req',     'generate an NDN certificate signing request',  None, 1),
    ('manpages/ndnsec-unlock-tpm',   'ndnsec-unlock-tpm',   'unlock the TPM',                               None, 1),
    ('manpages/ndn-client.conf',     'ndn-client.conf',     'configuration file for NDN platform',          None, 5),
    ('manpages/ndn-log',             'ndn-log',             'ndn-cxx logging',                              None, 7),
]

# If true, show URL addresses after external links.
#man_show_urls = True


# -- Custom options ----------------------------------------------------------

doxylink = {
    'ndn-cxx': ('ndn-cxx.tag', 'doxygen/'),
}

extlinks = {
    'issue': ('https://redmine.named-data.net/issues/%s', 'issue #'),
}

if os.getenv('GOOGLE_ANALYTICS', None):
    googleanalytics_id = os.environ['GOOGLE_ANALYTICS']
    googleanalytics_enabled = True

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import importlib.util
import sys

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'ndn-cxx: NDN C++ library with eXperimental eXtensions'
copyright = 'Copyright © 2013-2023 Regents of the University of California.'
author = 'Named Data Networking Project'

# The short X.Y version.
#version = ''

# The full version, including alpha/beta/rc tags.
#release = ''

# There are two options for replacing |today|: either, you set today to some
# non-false value, then it is used:
#today = ''
# Else, today_fmt is used as the format for a strftime call.
today_fmt = '%Y-%m-%d'


# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

needs_sphinx = '4.0'
extensions = [
    'sphinx.ext.extlinks',
    'sphinx.ext.todo',
]

def addExtensionIfExists(extension: str):
    try:
        if importlib.util.find_spec(extension) is None:
            raise ModuleNotFoundError(extension)
    except (ImportError, ValueError):
        sys.stderr.write(f'WARNING: Extension {extension!r} not found. '
                         'Some documentation may not build correctly.\n')
    else:
        extensions.append(extension)

addExtensionIfExists('sphinxcontrib.doxylink')

templates_path = ['_templates']
exclude_patterns = ['Thumbs.db', '.DS_Store']


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'named_data_theme'
html_theme_path = ['.']

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

html_copy_source = False
html_show_sourcelink = False

# Disable syntax highlighting of code blocks by default.
highlight_language = 'none'


# -- Options for manual page output ------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-manual-page-output

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    ('manpages/ndnsec',              'ndnsec',              'NDN security toolkit',                         [], 1),
    ('manpages/ndnsec-cert-dump',    'ndnsec-cert-dump',    'export an NDN certificate',                    [], 1),
    ('manpages/ndnsec-cert-gen',     'ndnsec-cert-gen',     'create an NDN certificate for an identity',    [], 1),
    ('manpages/ndnsec-cert-install', 'ndnsec-cert-install', 'import an NDN certificate from a file',        [], 1),
    ('manpages/ndnsec-delete',       'ndnsec-delete',       'delete an NDN identity, key, or certificate',  [], 1),
    ('manpages/ndnsec-export',       'ndnsec-export',       'export an NDN certificate and its private key to a file', [], 1),
    ('manpages/ndnsec-get-default',  'ndnsec-get-default',  'show the default NDN identity, key, and certificate for the current user', [], 1),
    ('manpages/ndnsec-import',       'ndnsec-import',       'import an NDN certificate and its private key from a file', [], 1),
    ('manpages/ndnsec-key-gen',      'ndnsec-key-gen',      'generate an NDN key for an identity',          [], 1),
    ('manpages/ndnsec-list',         'ndnsec-list',         'list all known NDN identities, keys, and certificates', [], 1),
    ('manpages/ndnsec-set-default',  'ndnsec-set-default',  'change the default NDN identity, key, or certificate for the current user', [], 1),
    ('manpages/ndnsec-sign-req',     'ndnsec-sign-req',     'generate an NDN certificate signing request',  [], 1),
    ('manpages/ndnsec-unlock-tpm',   'ndnsec-unlock-tpm',   'unlock the TPM',                               [], 1),
    ('manpages/ndn-client.conf',     'ndn-client.conf',     'configuration file for NDN applications',      [], 5),
    ('manpages/ndn-log',             'ndn-log',             'ndn-cxx logging',                              [], 7),
]


# -- Misc options ------------------------------------------------------------

doxylink = {
    'ndn-cxx': ('ndn-cxx.tag', 'doxygen/'),
}

extlinks = {
    'issue': ('https://redmine.named-data.net/issues/%s', 'issue #%s'),
}

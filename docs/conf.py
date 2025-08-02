# filepath: /Users/gregmolskow/AI/smartFountan/src/lib/SmartHomeUtils/docs/conf.py
import os
import sys
sys.path.insert(0, os.path.abspath('../GMSmartHomeDevice'))

project = 'Smart Home Device'
author = 'Greg Molskow'
release = '1.0'

extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'breathe',
]

breathe_projects = {
    "Smart Home Device": "doxygen/xml"
}
breathe_default_project = "Smart Home Device"

templates_path = ['_templates']
exclude_patterns = []

html_theme = 'alabaster'
html_static_path = ['_static']
html_logo = '_static/gm_logo.png'
html_favicon = '_static/gm_logo_bkgrn.png'
html_title = 'Smart Home Utilities Documentation'

# Add a copyright notice
html_context = {
    'display_github': True,
    'github_user': 'gregmolskow',
    'github_repo': 'SmartHomeUtils',
    'github_version': 'main/docs/',
    'copyright': '2025, Greg Molskow'
}


def setup(app):
    app.add_css_file('custom.css')


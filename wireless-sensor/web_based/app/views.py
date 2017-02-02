import logging
from flask import render_template, request, jsonify

from app import app

logger = logging.getLogger('root')


@app.route('/')
def index():
    return render_template("index.html")
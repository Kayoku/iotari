"""Init file for the wireless sensor monitoring app."""

from flask import Flask
from app.database.models import db

import flask.ext.restless


def create_app(config):
    """App factory to specify the configuratin object."""
    app = Flask(__name__)
    app.config.from_object(config)

    db.init_app(app)
    with app.app_context():
        db.create_all()
    return app

app = create_app('config')

from app import views
from app.database import models

# flask restless for API
with app.app_context():
    manager = flask.ext.restless.APIManager(app, flask_sqlalchemy_db=db)
    manager.create_api(models.Sensor, methods=['GET', 'POST', 'DELETE'])
    manager.create_api(models.Measure, methods=['GET', 'POST', 'DELETE'])

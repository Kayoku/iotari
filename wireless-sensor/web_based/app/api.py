import flask
from flask import render_template

import flask.ext.sqlalchemy
import flask.ext.restless

# Create the Flask application and the Flask-SQLAlchemy object.
app = flask.Flask(__name__, template_folder="front_end/templates")
app.config['DEBUG'] = True
app.config['SQLALCHEMY_DATABASE_URI'] = r'sqlite://'
db = flask.ext.sqlalchemy.SQLAlchemy(app)


class Measure(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    time_stamp = db.Column(db.DateTime, unique=True)
    value = db.Column(db.Float)

    sensor = db.relationship('Sensor', back_populates='measures')
    sensor_id = db.Column(db.Unicode, db.ForeignKey('sensor.id'))


class Sensor(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    location = db.Column(db.Unicode, unique=True)
    measures = db.relationship('Measure', back_populates='sensor')


@app.route("/")
def index():
    return render_template('index.html')


# Create the database tables.
db.create_all()

# Create the Flask-Restless API manager.
manager = flask.ext.restless.APIManager(app, flask_sqlalchemy_db=db)

# Create API endpoints, which will be available at /api/<tablename> by
# default. Allowed HTTP methods can be specified as well.
manager.create_api(Measure, methods=['GET', 'POST'])
manager.create_api(Sensor, methods=['GET', 'POST'])

# start the flask loop
app.run()

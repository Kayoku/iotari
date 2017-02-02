"""Database models using sqlalchemy.
The models are :
 * Measure with the measured values
 * Sensor with the sensors descriptions and with several measures
"""

import logging

from sqlalchemy import Column, Unicode, DateTime, Integer, Float, ForeignKey
from sqlalchemy.orm import relationship
from flask_sqlalchemy import SQLAlchemy

db = SQLAlchemy()

logger = logging.getLogger('root')


class Measure(db.Model):
    __tablename__ = 'measure'

    id = Column(Integer, primary_key=True)
    time_stamp = Column(DateTime, unique=True)
    value = Column(Float)

    sensor = relationship('Sensor', back_populates='measures')
    sensor_id = Column(Unicode, ForeignKey('sensor.id'))


class Sensor(db.Model):
    __tablename__ = 'sensor'

    id = Column(db.Integer, primary_key=True)
    location = Column(Unicode, unique=True)
    measures = relationship('Measure', back_populates='sensor')


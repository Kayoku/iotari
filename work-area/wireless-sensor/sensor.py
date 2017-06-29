import datetime
import json
from pprint import pprint
import requests


class Sensor():
    """Abstract sensor class."""

    def __init__(self, uuid):
        """Initialisation."""
        # find a way to get a stable name
        self.uuid = uuid

    def save_measure(self):
        """How to save a new measure."""
        raise NotImplementedError


class APISensor(Sensor):
    """Save a sensor value using a remote API."""

    HTTP_STATUS_CREATED = 201
    HTTP_STATUS_SUCCESS = 200

    def __init__(self, uuid, baseurl):
        """Initialize."""
        super().__init__(uuid)
        self.baseurl = baseurl
        self.uuid = uuid
        self.get_id()

    def get_id(self):
        """Get the database id for the sensor.

        If the sensor doesn't exist, it creates it.
        """
        filters = [dict(name='location', op='equals', val=self.uuid)]
        params = dict(q=json.dumps(dict(filters=filters)))
        r = requests.get(self.baseurl + '/api/sensor',
                         params=params,
                         headers={'content-type': 'application/json'})
        if r.status_code == self.HTTP_STATUS_SUCCESS:
            json_content = json.loads(r.text)
            if json_content["num_results"] == 1:
                self.id_ = json_content["objects"][0]["id"]
            elif json_content["num_results"] == 0:
                # add a new sensor in db with the UUID
                r = requests.post(baseurl + '/api/sensor',
                                  data=json.dumps({"location": self.uuid}),
                                  headers={'content-type': 'application/json'})
                if r.status_code == self.HTTP_STATUS_CREATED:
                    self.id_ = json.loads(r.text)["id"]
                else:
                    raise Exception("impossible to add new sensor")
            else:
                raise Exception("mulltiple sensors with same id")

    def save_measure(self, measure, time_stamp):
        new_mesure = {'value': measure,
                      'sensor_id': self.id_,
                      'time_stamp': time_stamp}
        try:
            r = requests.post(self.baseurl + '/api/measure',
                              data=json.dumps(new_mesure),
                              headers={'content-type': 'application/json'})
        except requests.exceptions.ConnectionError:
            return False

        return r.status_code == self.HTTP_STATUS_CREATED


if __name__ == "__main__":
    baseurl = 'http://localhost:5000'
    sensor = APISensor("salon", baseurl)

    for _ in range(50):
        sensor.save_measure(_, datetime.datetime.now().isoformat())

    r = requests.get(baseurl + '/api/sensor',
                     headers={'content-type': 'application/json'})
    print("Sensors: ")
    pprint({"status": r.status_code, "headers": r.headers['content-type'], "content": json.loads(str(r.text))})

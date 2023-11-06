""" This module contains the flask app """
from os import environ
from dotenv.main import load_dotenv
from flask import (
    Flask,
    make_response,
    jsonify,
)
from flask_cors import CORS
from models import storage
from api.views import app_views


app = Flask(__name__)
app.config['SECRET_KEY'] = environ.get('SECRET_KEY')
origins = '*'


CORS(
    app,
    resources={r"/*": {"origins": origins}},
    supports_credentials=True
)


app.register_blueprint(app_views)


@app.teardown_appcontext
def close_db(err):
    """ Closes the storage session on an error """
    storage.close()


@app.errorhandler(404)
def not_found(err):
    """ Handles 404 errors """
    response = make_response(jsonify({'Error': 'Not Found'}), 404)
    return response


if __name__ == '__main__':
    load_dotenv()
    app.run(
        host='0.0.0.0',
        port=environ.get('VEDA_PORT', default='8080'),
        threaded=True)

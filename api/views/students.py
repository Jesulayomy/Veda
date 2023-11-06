""" This module contains the student view """
import os

from flask import (
    jsonify,
    request,
    abort,
    make_response
)

from api.views import app_views
from models import storage
from models.student import Student


@app_views.route('/students', methods=['GET'], strict_slashes=False)
def get_students():
    """ Returns all students """
    students = storage.all('Student')
    return jsonify([user.to_dict() for user in students.values()])


@app_views.route('/students/<student_id>', methods=['GET'], strict_slashes=False)
def get_student(student_id):
    """ Returns a student """
    student = storage.get('Student', student_id)
    if not student:
        abort(404, 'Student does not exist')
    return jsonify(student.to_dict())


@app_views.route('/students/<student_id>', methods=['DELETE'], strict_slashes=False)
def delete_student(student_id):
    """ Deletes a student """
    student = storage.get('Student', student_id)
    if not student:
        abort(404, 'Student does not exist')
    storage.delete(student)
    return jsonify(student.to_dict()), 200


@app_views.route('/students', methods=['POST'], strict_slashes=False)
def post_student():
    """ Creates a new student """
    data = request.get_json()
    if not data:
        abort(400, 'Not a JSON')
    if 'id' not in data:
        abort(400, 'Missing id')
    student = Student(**data)
    storage.add(student)
    response = make_response(jsonify(student.to_dict()), 201)
    return response


@app_views.route('/students', methods=['PUT'], strict_slashes=False)
def put_student():
    """ Updates a student """
    data = request.get_json()
    if not data:
        abort(400, 'Not a JSON')
    if 'id' not in data:
        abort(400, 'Missing id')
    student = storage.get('Student', data['id'])
    if not student:
        print("Student does not exist")
        abort(404, 'Student does not exist')
    for key, value in data.items():
        setattr(student, key, value)
    storage.add(student)

    return jsonify(student.to_dict()), 200


@app_views.route('/students', methods=['DELETE'], strict_slashes=False)
def delete_all_students():
    """ Deletes a student """
    storage.drop()
    return jsonify({}), 200


@app_views.route(
    '/students/<student_id>/attendance',
    methods=['GET'],
    strict_slashes=False)
def get_student_attendance(student_id):
    """ Returns all attendance for a student """
    student = storage.get('Student', student_id)
    if not student:
        abort(404, 'Student does not exist')
    with storage.session_scope() as session:
        student = session.merge(student)
        attendance = [attendance.to_dict() for attendance in student.attendances]

    return jsonify(attendance)

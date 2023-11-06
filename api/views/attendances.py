""" This module handles all the routes for the attendance endpoint """
from datetime import datetime
from flask import (
    jsonify,
    request,
    abort,
    make_response
)

from api.views import app_views
from models import storage
from models.student import Student
from models.attendance import Attendance


@app_views.route('/attend', methods=['POST'], strict_slashes=False)
def new_attend():
    """ Creates a new attendance """
    data = request.get_json()
    if not data:
        abort(400, 'Not a JSON')
    if 'student_id' not in data and 'id' not in data:
        abort(400, 'Missing student_id')
    if 'student_id' not in data:
        id = data['id']
        del data['id']
        data['student_id'] = id
    else:
        id = data['student_id']
    student = storage.get('Student', id)
    if not student:
        enroll = {'id': data["student_id"] if 'student_id' in data else data["id"]}
        student = Student(**enroll)
        storage.add(student)
    data['present'] = True
    data['date'] = datetime.utcnow().strftime('%Y-%m-%d')
    data['time'] = datetime.utcnow().strftime('%H:%M')
    attendance = Attendance(**data)
    storage.add(attendance)
    response = make_response(jsonify(student.to_dict()), 201)
    return response


@app_views.route('/attend', methods=['PUT'], strict_slashes=False)
def put_attend():
    """ Updates an attendance """
    data = request.get_json()
    if not data:
        abort(400, 'Not a JSON')
    if 'id' not in data:
        abort(400, 'Missing id')

    with storage.session_scope() as session:
        attendance = session.query(Attendance).filter(
            Attendance.id == data['id']
        ).first()
    if not attendance:
        abort(404, 'Record does not exist')
    for key, value in data.items():
        setattr(attendance, key, value)
    storage.add(attendance)
    response = make_response(jsonify(attendance.to_dict()), 201)
    return response


@app_views.route('/attendances', methods=['GET'], strict_slashes=False)
def attendance_sheet():
    """ Returns all students attendance """

    attendance_sheet = []
    students = storage.all('Student')
    for student in students.values():
        with storage.session_scope() as session:
            student = session.merge(student)
            attendance = [{
                'date': attendance.date,
                'time': attendance.time,
                'present': attendance.present
            } for attendance in student.attendances]
            attendance_sheet.append(
                {
                    'id': student.id,
                    'first_name': student.first_name,
                    'last_name': student.last_name,
                    'matric_number': student.matric_number,
                    'attendance': attendance
                }
            )
    return jsonify(attendance_sheet), 200

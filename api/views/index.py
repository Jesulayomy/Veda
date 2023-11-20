""" This module contains the index view """
import csv

from api.views import app_views
from flask import jsonify, render_template, send_file
from models import storage
from api.views.attendances import attendance_sheet


@app_views.route('/status', methods=['GET'])
def status():
    """ Returns the status of the API """
    return jsonify({'status': 'OK'})


@app_views.route('/stats', methods=['GET'], strict_slashes=False)
def stats():
    """ Returns the stats of the API """
    return jsonify({
        'students': storage.count('Student'),
        'attendances': storage.count('Attendance'),
    })


@app_views.route('/', methods=['GET'], strict_slashes=False)
def index():
    """ Returns the index of the API """

    sheets = attendance_sheet()[0].json
    
    unique_dates = set()
    
    # Loop through the sheets and add dates to the set
    for sheet in sheets:
        for attendance in sheet['attendance']:
            unique_dates.add(attendance['date'])

    return render_template('index.html', sheets=sheets, unique_dates=sorted(unique_dates))


@app_views.route('/sheet', methods=['GET'], strict_slashes=False)
def sheet():
    """ Returns the sheet as a csv to download """

    sheets = attendance_sheet()[0].json
    csv_file = "attendance.csv"

    # Write JSON data to a CSV file
    with open(csv_file, mode='w', newline='') as file:
        writer = csv.writer(file)

        # Write header row
        dates = set()
        for student in sheets:
            for attendance in student["attendance"]:
                dates.add(attendance["date"])
        dates = sorted(dates)
        rows = ["id", "first_name", "last_name", "matric_number"]
        rows.extend(dates)
        end = ["score", "percentage"]
        rows.extend(end)
        writer.writerow(rows)

        # Iterate through the JSON data and write to the CSV file
        for student in sheets:
            total = 0
            newRow = [student["id"], student["first_name"], student["last_name"], student["matric_number"]]
            for date in dates:
                for record in student["attendance"]:
                    if record["date"] == date:
                        newRow.append(1)
                        total += 1
                        break
                else:
                    newRow.append(0)
            newRow.append(total)
            percentage = (total / len(dates)) * 100
            newRow.append(round(percentage, 2))

            writer.writerow(newRow)

    return send_file(csv_file, as_attachment=True)

""" Attendace model """
from datetime import datetime
from sqlalchemy import (
    String,
    Column,
    Integer,
    DateTime,
    Boolean,
    ForeignKey,
)
from sqlalchemy.orm import relationship

from typing import Any
from models.base import (
    BaseModel,
    Base,
)


class Attendance(BaseModel, Base):
    __tablename__ = 'attendances'

    id = Column(
        Integer,
        primary_key=True,
        nullable=False,
        autoincrement=True
    )

    student_id = Column(Integer, ForeignKey('students.id'))
    time = Column(String(10))
    date = Column(String(10))  # You can use a more suitable date type like Date
    present = Column(Boolean, default=False)

    student = relationship('Student', back_populates='attendances')

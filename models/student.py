""" This module contains the User class """
""" REMWEMBER SIGNIN TIME IN TO_DICT """
from datetime import datetime
from sqlalchemy import (
    String,
    Column,
    Integer,
    DateTime,
)
from sqlalchemy.orm import relationship

from typing import Any
from models.base import (
    BaseModel,
    Base,
)


class Student(BaseModel, Base):
    """ Student class """

    __tablename__ = "students"

    id = Column(
        Integer,
        primary_key=True,
        nullable=False,
    )

    first_name = Column(
        String(63),
        nullable=True
    )

    last_name = Column(
        String(63),
        nullable=True
    )

    matric_number = Column(
        String(63),
        nullable=True
    )

    attendances = relationship(
        'Attendance',
        back_populates='student',
        cascade='all, delete-orphan'
    )

    def __setattr__(self, name: str, value: Any) -> None:
        """ Sets attributes for the user class """

        if name == "first_name" or name == "last_name":
            if not value.isalpha():
                raise ValueError("Names must contain only letters")

        super().__setattr__(name, value)

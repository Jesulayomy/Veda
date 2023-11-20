""" This module contains the Base class to inherit for other classes """
from datetime import datetime
from sqlalchemy import (
    Column,
    DateTime,
)
from sqlalchemy.orm import declarative_base
from typing import (
    Dict,
    Any,
)


Base = declarative_base()


class BaseModel:
    """ The base class for all db classes """

    created_at = Column(
        DateTime,
        default=datetime.utcnow(),
        nullable=False
    )

    def __init__(self, **kwargs) -> None:
        """ Initializes the basemodel class  """

        if kwargs:
            for key, value in kwargs.items():
                if key not in ["created_at"]:
                    setattr(self, key, value)

        self.created_at = datetime.utcnow()

    def to_dict(self) -> Dict[str, Any]:
        """ Returns the dictionary representation of the implied class """

        self_dict = {}
        for key, val in self.__dict__.items():
            if key == "created_at":
                pass
            else:
                self_dict[key] = val

        if "_sa_instance_state" in self_dict:
            del self_dict["_sa_instance_state"]

        return self_dict

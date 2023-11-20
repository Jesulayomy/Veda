""" This module contains the storage class for the database"""
from contextlib import contextmanager
from dotenv.main import load_dotenv
from os import environ
from sqlalchemy import (
    create_engine,
)
from sqlalchemy.orm import (
    sessionmaker,
    scoped_session,
)
from sqlalchemy.pool import QueuePool
from typing import (
    Dict,
    Type,
    Union,
)
from models.attendance import Attendance
from models.base import Base
from models.student import Student


load_dotenv()


class Storage:
    """ database storage class """

    __engine = None

    def __init__(self, **kwargs):
        """ initializes storage class """

        uname = environ["DB_VEDA_USER"]
        passwd = environ["DB_VEDA_PASSWORD"]
        dbname = environ["DB_VEDA_NAME"]
        dbhost = environ["DB_VEDA_HOST"]
        if environ["DB_VEDA_MODE"] == "test" or kwargs.get("test", False):
            dbname = environ["DB_VEDA_TEST_NAME"]
        """ Remove echo when done testing """
        self.__engine = create_engine(
            'mysql+mysqlconnector://{}:{}@{}/{}'.format(
                uname, passwd, dbhost, dbname),
            pool_pre_ping=True,
            poolclass=QueuePool,
            pool_size=10)
        self.session_factory = sessionmaker(bind=self.__engine,
                                            expire_on_commit=False)
        self.Session = scoped_session(self.session_factory)
        if environ["DB_VEDA_MODE"] == "test" or kwargs.get("test", False):
            Base.metadata.drop_all(self.__engine, checkfirst=True)

    def reload(self) -> None:
        """ Reloads the session and create tables """

        Base.metadata.create_all(self.__engine)

    @contextmanager
    def session_scope(self) -> None:
        """
            Creates a session, and tearsDown after control
            is transferred back
        """

        session = self.Session()
        try:
            yield session
            session.commit()
        except Exception:
            session.rollback()
            raise Exception
        finally:
            session.close()

    def all(
        self,
        cls: Union[str, Type[Student], Type[Attendance]] = None
    ) -> Dict[str, Student]:
        """ gets all objects or all of a specific class """

        objects = {}
        if cls:
            if type(cls) is str:
                cls = eval(cls)
            with self.session_scope() as session:
                query = session.query(cls)
                for obj in query:
                    key = "{}.{}".format(obj.__class__.__name__, obj.id)
                    objects[key] = obj
        else:
            for model in [Student, Attendance]:
                with self.session_scope() as session:
                    query = session.query(model)
                    for obj in query:
                        key = "{}.{}".format(obj.__class__.__name__, obj.id)
                        objects[key] = obj

        return objects

    def get(
            self,
            cls: Union[str, Type[Student], Type[Attendance]],
            id: Union[str, int]) -> Student:
        """ gets a particular object """

        if type(cls) is str:
            cls = eval(cls)

        with self.session_scope() as session:
            try:
                if cls is Student:
                    obj = session.query(cls).filter(cls.id == id).one()
                elif cls is Attendance:
                    obj = session.query(cls).filter(cls.date == id).one()
            except Exception:
                obj = None

        return obj

    def count(self, cls: Union[str, Type[Student], Type[Attendance]] = None) -> int:
        """ Returns the number of objects of a class """

        with self.session_scope() as session:
            if cls:
                if type(cls) is str:
                    cls = eval(cls)
                return session.query(cls).count()
            else:
                count = session.query(Student).count()
                return count

    def add(self, obj: Student) -> None:
        """ Adds an object to the current session """

        with self.session_scope() as session:
            session.add(obj)

    def save(self) -> None:
        """ Commits the current session - Used in lingering sessions """

        with self.session_scope() as session:
            session.commit()

    def delete(self, obj: Union[Student, Attendance]) -> None:
        """ deletes an object from the current session """

        with self.session_scope() as session:
            session.delete(obj)

    def close(self) -> None:
        """ removes the current session """

        self.Session.remove()

    def drop(self) -> None:
        """ drops all tables """

        Base.metadata.drop_all(self.__engine, checkfirst=True)
        Base.metadata.create_all(self.__engine)

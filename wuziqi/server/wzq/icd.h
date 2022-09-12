#pragma once

#include <boost/serialization/variant.hpp>
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>
// #include <boost/serialization/string.hpp>
// #include <boost/serialization/vector.hpp>
// #include <boost/serialization/access.hpp>


enum icd{requese_=1,success_,dropPoint_,keepAlive_,reply_,ifWin_,name_};

class base{
public:
	base(int a):id_(a){}
	base(){}
    int id_;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & id_;
	}
};

inline static std::ostream& operator<<(std::ostream& os,const base& r){
    return os<<r.id_<<std::endl;
}

class requese:public::base{
public:
	requese(int a,int b):base(a),res_(b){}
	requese(){}
    int res_;  //if receive the requese to client, requese to server;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<base>(*this);
		ar & res_;
	}
};
inline static std::ostream& operator<<(std::ostream& os,const requese& r){
    return os<<r.id_<<" "<<r.res_<<std::endl;
}

class success:public::base{
public:
	success(int a,int b,int c,int d):base(a),isSuccess_(b),playerNumber(c),whoFirst(d){}
	success(){}
    int    isSuccess_;
    int    playerNumber;
    int    whoFirst;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<base>(*this);
		ar & isSuccess_;
        ar & playerNumber;
        ar & whoFirst;
	}
};

inline static std::ostream& operator<<(std::ostream& os , const success& s){
    return os<<s.id_<<" "<<s.isSuccess_<<" "<<s.playerNumber<<" "<<s.whoFirst<<std::endl;
}

class dropPoint:public::base{
public:
	dropPoint(int a,int b,int c,int d,int e):base(a),gameId(b),player(c),x(d),y(e){}
	dropPoint(){}
    int gameId;
    int player;
    int x;
    int y;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<base>(*this);
		ar & gameId;
        ar & player;
        ar & x;
        ar & y;
	}
};

inline static std::ostream& operator<<(std::ostream& os ,const dropPoint& d){
    return os<<d.id_<<" "<<d.gameId<<" "<<d.player<<" "<<d.x<<" "<<d.y<<" "<<std::endl;
}

class keepAlive:public::base{
public:
	keepAlive(int a,int b):base(a),dummy(b){}
	keepAlive(){}
    int dummy;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<base>(*this);
		ar & dummy;
	}
};

inline static std::ostream& operator<<(std::ostream& os ,const keepAlive& d){
    return os<<d.id_<<" "<<d.dummy<<" "<<std::endl;
}


class reply:public::base{
public:
	reply(int a,int b,int c):base(a),replyIcd(*(icd*)&b),isSu(c){}
	reply(){}
    icd replyIcd;
	int isSu;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<base>(*this);
		ar & replyIcd;
	}
};
inline static std::ostream& operator<<(std::ostream& os ,const reply& d){
    return os<<d.id_<<" "<<d.replyIcd<<" "<<d.isSu<<" "<<std::endl;
}

class ifWin:public::base{
public:
	ifWin(int a,int b):base(a),ifw(b){}
	ifWin(){}
    int ifw;
private:
	friend class boost::serialization::access;
 
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		// serialize base class information
		ar & boost::serialization::base_object<base>(*this);
		ar & ifw;
	}
};
inline static std::ostream& operator<<(std::ostream& os ,const ifWin& d){
    return os<<d.id_<<" "<<d.ifw<<" "<<std::endl;
}

class name:public::base{
public:
	name(int a,std::string s1):base(a),s(s1){}
	name(){}
	std::string s;
private:
    friend class boost::serialization::access;
    template <class Ar> void serialize(Ar& ar, unsigned) {
		ar & boost::serialization::base_object<base>(*this);
        ar & s;
    }
};
inline static std::ostream& operator<<(std::ostream& os ,const name& d){
    return os<<d.id_<<" "<<d.s<<" "<<std::endl;
}


class FileContents { // conventions...
public:
    boost::variant<requese,success,dropPoint,keepAlive,reply,ifWin,name> payload;
	// boost::variant<requese> payload;
  private:
    friend class boost::serialization::access;
    template <class Ar> void serialize(Ar& ar, unsigned) {
        ar & payload;
    }
};
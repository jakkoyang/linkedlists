#include "moviedb.h"

using namespace std;

void MovieDatabase::insert(Movie e, Node* node){ //add before Node* node
    Node* add = new Node;
    add->elem = e;
    add->next = node;
    add->prev = node->prev;
    node->prev->next = add;
    node->prev = add;
    count++;
}

void MovieDatabase::del(Node* node){ //delete node
    node->prev->next = node->next;
    node->next->prev = node->prev;
    delete node;
    count--;
}

MovieDatabase::MovieDatabase()
{
    header->next = trailer;
    header->prev = nullptr;
    trailer->prev = header;
    trailer->next = nullptr;
}

MovieDatabase::~MovieDatabase()
{
    while(!isEmpty()){
        removeFront();
    }
}

bool MovieDatabase::isEmpty() const
{
    return (header->next == trailer && trailer->prev == header);
}

int MovieDatabase::getSize() const 
{
    return count;
}

const Movie& MovieDatabase::getFront() const
{
    if(isEmpty()){ 
        throw std::runtime_error("Empty list"); 
    }
    return header->next->elem;
}

const Movie& MovieDatabase::getBack() const
{
    if(isEmpty()){ 
        throw std::runtime_error("Empty list"); 
    }
    return trailer->prev->elem;
}

void MovieDatabase::addFront(const Movie& movie)
{
    insert(movie, header->next);
}

void MovieDatabase::addBack(const Movie& movie)
{
    insert(movie, trailer);
}

void MovieDatabase::removeFront()
{
    if(isEmpty()){
        throw std::runtime_error("Empty list");
    }
    del(header->next);
}

void MovieDatabase::removeBack()
{
    if(isEmpty()){
        throw std::runtime_error("Empty list");
    }
    del(trailer->prev);
}

void MovieDatabase::append(const MovieDatabase& otherDB, function<bool(const Movie&)> predicate)
{
    Node* current = otherDB.header;
    while(current != nullptr){
        if(predicate(current->elem)){
            addBack(current->elem);
        }
    current = current->next;
    }
}

void MovieDatabase::load(const std::string& filename)
{
    io::CSVReader<2> in(filename);
    in.read_header(io::ignore_extra_column, "Title", "Year");
    string title;
    int year;
    Movie movie;  //struct setup
    while (in.read_row(title, year)) {
        movie.title = title;
        movie.year = year;
        addBack(movie);
    }
}

void MovieDatabase::beginTransaction()
{
    transaction = true;
    cursor = header;
}

void MovieDatabase::next()
{
    if(cursor == trailer || isEmpty()){
        throw std::runtime_error("past last element");
    }
    cursor = cursor->next;
}

void MovieDatabase::previous()
{
    if(cursor == header || isEmpty()){
        throw std::runtime_error("before first element");
    }
    cursor = cursor->prev;
}

void MovieDatabase::insert(const Movie& movie)
{
    if(!transaction){
        throw std::runtime_error("not in transaction mode");
    }
    if(cursor == trailer){
        throw std::runtime_error("past last element");
    }
    insert(movie, cursor->next);
    next();
}

void MovieDatabase::remove()
{
    if(!transaction){
        throw std::runtime_error("not in transaction mode");
    }
    if(cursor == trailer){
        throw std::runtime_error("past last element");
    }
    if(cursor == header){
        throw std::runtime_error("past first element");
    }
    cursor = cursor->next;
    del(cursor->prev);
}

const Movie& MovieDatabase::getCurrent() const
{
    if(!transaction){
        throw std::runtime_error("not in transaction mode");
    }
    if(cursor == trailer){
        throw std::runtime_error("past last element");
    }
    if(cursor == header){
        throw std::runtime_error("past first element");
    }
    return cursor->elem;
}

void MovieDatabase::endTransaction()
{
    transaction = false;
    cursor = nullptr;
}

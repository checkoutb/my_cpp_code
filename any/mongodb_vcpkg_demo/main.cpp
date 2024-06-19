#include <cstdint>
#include <iostream>
#include <vector>
#include <cassert>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;


// https://www.mongodb.com/docs/languages/cpp/cpp-driver/current/tutorial/#std-label-cpp-tutorial

int main() {

    std::cout<<"111111"<<std::endl;

    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::client client{mongocxx::uri{}};

    auto db = client["mydb"];
    auto collection = db["test"]; // db,collection 不存在就create

////////////////////////
    // 这个 doc_value 没有 insert ，所以 数据库里没有。
    // 确实，文章的 节 是 create a document， 之前还在疑惑，和下面 insert document 有什么区别。
    //      结果，这里真的只是 create， 没有insert。。
    auto doc_value = make_document(
        kvp("name", "MongoDB"),
        kvp("type", "database"),
        kvp("count", 1),
        kvp("versions", make_array("v6.0", "v5.0", "v4.4", "v4.2", "v4.0", "v3.6")),
        kvp("info", make_document(kvp("x", 203), kvp("y", 102)))
    );

    auto doc_view = doc_value.view();

    auto element = doc_view["name"];
    assert(element.type() == bsoncxx::type::k_string);
    auto name = element.get_string().value; // For C++ driver version < 3.7.0, use get_utf8()
    assert(0 == name.compare("MongoDB"));




////////////////////// 
    // insert document
    std::cout<<"22222, start insert"<<std::endl;


    auto insert_one_result = collection.insert_one(make_document(kvp("i", 0)));

    assert(insert_one_result);

    auto doc_id = insert_one_result->inserted_id();
    assert(doc_id.type() == bsoncxx::type::k_oid);

/////////// insert multiple docs
    std::vector<bsoncxx::document::value> documents;
    documents.push_back(make_document(kvp("i", 1)));
    documents.push_back(make_document(kvp("i", 2)));

    auto insert_many_result = collection.insert_many(documents);
    assert(insert_many_result);  // Acknowledged writes return results.

    auto doc0_id = insert_many_result->inserted_ids().at(0);
    auto doc1_id = insert_many_result->inserted_ids().at(1);
    assert(doc0_id.type() == bsoncxx::type::k_oid);
    assert(doc1_id.type() == bsoncxx::type::k_oid);


//////////////////////
    // query document
    std::cout<<"333333333, start query"<<std::endl;

    auto find_one_result = collection.find_one({}); // std::optional<bsoncxx::document::value>
    if (find_one_result) {
        // Do something with *find_one_result
        std::cout<<" find one 111111"<<std::endl;
    } else {
        std::cout<<" fine none 00000"<<std::endl;
    }
    assert(find_one_result);


///////////// find all

    auto cursor_all = collection.find({});
    std::cout << "collection " << collection.name()
            << " contains these documents:" << std::endl;
    for (auto doc : cursor_all) {
        assert(doc["_id"].type() == bsoncxx::type::k_oid);
        std::cout << bsoncxx::to_json(doc, bsoncxx::ExtendedJsonMode::k_relaxed) << std::endl;
    }
    std::cout << std::endl;


///////////// find by param

    auto find_one_filtered_result = collection.find_one(make_document(kvp("i", 0)));
    if (find_one_filtered_result) {
        // Do something with *find_one_filtered_result
        //std::cout<<" find ooooo by param "<<find_one_filtered_result["_id"]<<std::endl;
    }


    auto cursor_filtered =
        collection.find(make_document(kvp("i", make_document(kvp("$gt", 0), kvp("$lte", 2)))));
    for (auto doc : cursor_filtered) {
        // Do something with doc
        assert(doc["_id"].type() == bsoncxx::type::k_oid);
    }



    
/////////////////////
    // update document
    std::cout<<"44444444444  update doc"<<std::endl;

    auto update_one_result = collection.update_one(make_document(kvp("i", 0)), make_document(kvp("$set", make_document(kvp("foo", "bar")))));
    assert(update_one_result);  // Acknowledged writes return results.
    assert(update_one_result->modified_count() == 1);

////// update multi docs
    auto update_many_result =
        collection.update_many(make_document(kvp("i", make_document(kvp("$gt", 0)))),
                                make_document(kvp("$set", make_document(kvp("foo", "buzz")))));
    assert(update_many_result);  // Acknowledged writes return results.
    assert(update_many_result->modified_count() == 2);



///////////////////
    // delete document
    std::cout<<"5555555555555 delete doc"<<std::endl;

// 这个只删除一个， 现有 2个 i=0 的，它只删除一个。  所以 这里 assert 是1， 并且 代码执行完后， 数据库里还有一条 i=0的。  估计删除 老的那条
// 
    auto delete_one_result = collection.delete_one(make_document(kvp("i", 0)));
    assert(delete_one_result);  // Acknowledged writes return results.
    assert(delete_one_result->deleted_count() == 1);


///////// delete all/multi docs
    auto delete_many_result =
        collection.delete_many(make_document(kvp("i", make_document(kvp("$gt", 0)))));
    assert(delete_many_result);  // Acknowledged writes return results.
    assert(delete_many_result->deleted_count() == 2);


////////////////////
    // create index
    std::cout<<"66666666666 create index"<<std::endl;
    auto index_specification = make_document(kvp("i", 1));
    collection.create_index(std::move(index_specification));



    std::cout<<"eeeennnnnddddd"<<std::endl;
}

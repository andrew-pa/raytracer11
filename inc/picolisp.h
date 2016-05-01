#pragma once

#include "picojson.h"

namespace picojson {
	struct lisp_functor {

		value body;
		vector<string> arg_names;
		map<string, value>* closure;

		lisp_functor(value b, vector<string> an) : body(b), arg_names(an), closure(nullptr) {}
		lisp_functor(value b, vector<string> an, map<string, value>* c) : body(b), arg_names(an), closure(c) {}


		value operator()(context& cx2, const std::vector<value>& args) {
			cx2.scopes.push_front(closure != nullptr ? *closure : map<string,value>{});
			for (int i = 0; i < args.size() && i < arg_names.size(); ++i) {
				cx2.name_value(arg_names[i], args[i]);
			}
			auto res = resolve(body, cx2);
			cx2.scopes.pop_front();
			return res;
		}
	};

	void init_cmmn_lisp(context& cx) {
		cx.name_value("+", value(vfunc([](context& cx, const std::vector<value>& args) { 
			double v = 0.;
			for (const auto& a : args) v += resolve(a,cx).get<double>();
			return value(v);
		})));
		cx.name_value("-", value(vfunc([](context& cx, const std::vector<value>& args) {
			double v = resolve(args[0], cx).get<double>();
			for (int i = 1; i < args.size(); ++i) v -= resolve(args[i], cx).get<double>();
			return value(v);
		})));
		cx.name_value("*", value(vfunc([](context& cx, const std::vector<value>& args) {
			double v = resolve(args[0], cx).get<double>();
			for (int i = 1; i < args.size(); ++i) v *= resolve(args[i], cx).get<double>();
			return value(v);
		})));
		cx.name_value("/", value(vfunc([](context& cx, const std::vector<value>& args) {
			return value(resolve(args[0], cx).get<double>()/ resolve(args[1], cx).get<double>());
		})));
		cx.name_value("defvar", value(vfunc([](context& cx, const std::vector<value>& args) {
			cx.name_value(*args[0].get<id>(), resolve(args[1], cx));
			return value(null_type,false);
		})));
		cx.name_value("lambda", value(vfunc([](context& cx, const std::vector<value>& args) {
			vector<string> argnames;
			auto nx = &args[0].get<cons_cell>();
			int i = 0;
			while (nx != nullptr) {
				if (nx->first == nullptr) break;
				argnames.push_back(*nx->first->get<id>());
				if (nx->last == nullptr) break;
				nx = &nx->last->get<cons_cell>();
			}
			map<string, value>* cl = new map<string, value>();
			nx = &args[1].get<cons_cell>();
			while (nx != nullptr) {
				if (nx->first == nullptr) break;
				if (nx->first->is<id>()) {
					if (find(argnames.begin(), argnames.end(), *nx->first->get<id>()) == argnames.end())
						(*cl)[*nx->first->get<id>()] = cx.named_value(*nx->first->get<id>());
				}
				if (nx->last == nullptr) break;
				nx = &nx->last->get<cons_cell>();
			}
			return value(vfunc(lisp_functor(args[1], argnames, cl)));
		})));
		cx.name_value("defun", value(vfunc([](context& cx, const std::vector<value>& args) {
			//(defun *name* (*args*) (*body*)) 
			//(defun madd (x y z) (+ (* x y) z))
			vector<string> ans;
			auto nx = &args[1].get<cons_cell>();
			int i = 0;
			while (nx != nullptr) {
				if (nx->first == nullptr) break;
				ans.push_back(*nx->first->get<id>());
				if (nx->last == nullptr) break;
				nx = &nx->last->get<cons_cell>();
			}
			cx.name_value(*args[0].get<id>(), value(vfunc(lisp_functor(args[2], ans))));
			return value(null_type, false);
		})));
		cx.name_value("map", value(vfunc([](context& cx, const std::vector<value>& args) {
			auto F = resolve(args[0], cx).get<vfunc>();
			auto L = resolve(args[1], cx);
			if (L.is<array>()) {
				array rs;
				for (auto v : L.get<array>()) {
					rs.push_back(F(cx, { v }));
				}
				return value(rs);
			}
			else if (L.is<cons_cell>()) {
				cons_cell root;
				auto next = &root;
				auto nx = &L.get<cons_cell>();
				while (nx != nullptr) {
					if (nx->first == nullptr) break;
					next->first = make_shared<value>(F(cx, { *nx->first }));
					next->last = make_shared<value>(cons_type, false);
					next = &next->last->get<cons_cell>();
					if (nx->last == nullptr) break;
					nx = &nx->last->get<cons_cell>();
				}
				return value(root.first, root.last);
			}
			return value();
		})));
	}
}
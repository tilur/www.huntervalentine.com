<?php
// $Id: uc_order_handler_field_order_cost.inc,v 1.1 2010/11/24 15:18:30 islandusurper Exp $

/**
 * @file
 * Ordel total cost field handler
 */

class uc_order_handler_field_order_cost extends uc_order_handler_field_money_amount {

  function construct() {
    parent::construct();
    $this->additional_fields['order_id'] = 'order_id';
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
    //do nothing else with the query, we'll be retrieving the information from the db
  }

  function render($values) {
    $this->field_alias = 'order_cost';
    $cost = db_result(db_query("SELECT SUM(cost) FROM {uc_order_products} WHERE order_id = %d", $values->{$this->aliases['order_id']}));
    $values->{$this->field_alias} = $cost;
    return parent::render($values);
  }

}
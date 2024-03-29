<?php
// $Id: uc-order--admin.tpl.php,v 1.2 2010/08/25 13:40:25 islandusurper Exp $

/**
 * @file
 * This file is the default admin notification template for Ubercart.
 */
?>

<p>
<?php echo t('Order number:'); ?> <?php echo $order_admin_link; ?><br />
<?php echo t('Customer:'); ?> <?php echo $order_first_name; ?> <?php echo $order_last_name; ?> - <?php echo $order_email; ?><br />
<?php echo t('Order total:'); ?> <?php echo $order_total; ?><br />
<?php echo t('Shipping method:'); ?> <?php echo $order_shipping_method; ?>
</p>

<p>
<?php echo t('Products:'); ?><br />
<?php foreach ($products as $product) { ?>
- <?php echo $product->qty; ?> x <?php echo $product->title . ' - ' . uc_currency_format($product->price * $product->qty); ?><br />
&nbsp;&nbsp;<?php echo t('SKU: ') . $product->model; ?><br />
    <?php if (is_array($product->data['attributes']) && count($product->data['attributes']) > 0) { ?>
    <?php foreach ($product->data['attributes'] as $attribute => $option) {
      echo '&nbsp;&nbsp;' . t('@attribute: @options', array('@attribute' => $attribute, '@options' => implode(', ', (array)$option))) . '<br />';
    } ?>
    <?php } ?>
<br />
<?php } ?>
</p>

<p>
<?php echo t('Order comments:'); ?><br />
<?php echo $order_comments; ?>
</p>
